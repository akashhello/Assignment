/*
 * Store Windows ACLs in data store - common functions.
 * #included into modules/vfs_acl_xattr.c and modules/vfs_acl_tdb.c
 *
 * Copyright (C) Volker Lendecke, 2008
 * Copyright (C) Jeremy Allison, 2009
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

static NTSTATUS create_acl_blob(const struct security_descriptor *psd,
			DATA_BLOB *pblob,
			uint16_t hash_type,
			uint8_t hash[XATTR_SD_HASH_SIZE]);

static NTSTATUS get_acl_blob(TALLOC_CTX *ctx,
			vfs_handle_struct *handle,
			files_struct *fsp,
			const char *name,
			DATA_BLOB *pblob);

static NTSTATUS store_acl_blob_fsp(vfs_handle_struct *handle,
			files_struct *fsp,
			DATA_BLOB *pblob);

#define HASH_SECURITY_INFO (OWNER_SECURITY_INFORMATION | \
				GROUP_SECURITY_INFORMATION | \
				DACL_SECURITY_INFORMATION | \
				SACL_SECURITY_INFORMATION)

/*******************************************************************
 Hash a security descriptor.
*******************************************************************/

static NTSTATUS hash_sd_sha256(struct security_descriptor *psd,
			uint8_t *hash)
{
	DATA_BLOB blob;
	SHA256_CTX tctx;
	NTSTATUS status;

	memset(hash, '\0', XATTR_SD_HASH_SIZE);
	status = create_acl_blob(psd, &blob, XATTR_SD_HASH_TYPE_SHA256, hash);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	SHA256_Init(&tctx);
	SHA256_Update(&tctx, blob.data, blob.length);
	SHA256_Final(hash, &tctx);

	return NT_STATUS_OK;
}

/*******************************************************************
 Parse out a struct security_descriptor from a DATA_BLOB.
*******************************************************************/

static NTSTATUS parse_acl_blob(const DATA_BLOB *pblob,
				struct security_descriptor **ppdesc,
				uint16_t *p_hash_type,
				uint8_t hash[XATTR_SD_HASH_SIZE])
{
	TALLOC_CTX *ctx = talloc_tos();
	struct xattr_NTACL xacl;
	enum ndr_err_code ndr_err;
	size_t sd_size;

	ndr_err = ndr_pull_struct_blob(pblob, ctx, NULL, &xacl,
			(ndr_pull_flags_fn_t)ndr_pull_xattr_NTACL);

	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		DEBUG(5, ("parse_acl_blob: ndr_pull_xattr_NTACL failed: %s\n",
			ndr_errstr(ndr_err)));
		return ndr_map_error2ntstatus(ndr_err);;
	}

	switch (xacl.version) {
		case 2:
			*ppdesc = make_sec_desc(ctx, SEC_DESC_REVISION,
					xacl.info.sd_hs2->sd->type | SEC_DESC_SELF_RELATIVE,
					xacl.info.sd_hs2->sd->owner_sid,
					xacl.info.sd_hs2->sd->group_sid,
					xacl.info.sd_hs2->sd->sacl,
					xacl.info.sd_hs2->sd->dacl,
					&sd_size);
			/* No hash - null out. */
			*p_hash_type = XATTR_SD_HASH_TYPE_NONE;
			memset(hash, '\0', XATTR_SD_HASH_SIZE);
			break;
		case 3:
			*ppdesc = make_sec_desc(ctx, SEC_DESC_REVISION,
					xacl.info.sd_hs3->sd->type | SEC_DESC_SELF_RELATIVE,
					xacl.info.sd_hs3->sd->owner_sid,
					xacl.info.sd_hs3->sd->group_sid,
					xacl.info.sd_hs3->sd->sacl,
					xacl.info.sd_hs3->sd->dacl,
					&sd_size);
			*p_hash_type = xacl.info.sd_hs3->hash_type;
			/* Current version 3. */
			memcpy(hash, xacl.info.sd_hs3->hash, XATTR_SD_HASH_SIZE);
			break;
		default:
			return NT_STATUS_REVISION_MISMATCH;
	}

	TALLOC_FREE(xacl.info.sd);

	return (*ppdesc != NULL) ? NT_STATUS_OK : NT_STATUS_NO_MEMORY;
}

/*******************************************************************
 Create a DATA_BLOB from a security descriptor.
*******************************************************************/

static NTSTATUS create_acl_blob(const struct security_descriptor *psd,
			DATA_BLOB *pblob,
			uint16_t hash_type,
			uint8_t hash[XATTR_SD_HASH_SIZE])
{
	struct xattr_NTACL xacl;
	struct security_descriptor_hash_v3 sd_hs3;
	enum ndr_err_code ndr_err;
	TALLOC_CTX *ctx = talloc_tos();

	ZERO_STRUCT(xacl);
	ZERO_STRUCT(sd_hs3);

	xacl.version = 3;
	xacl.info.sd_hs3 = &sd_hs3;
	xacl.info.sd_hs3->sd = CONST_DISCARD(struct security_descriptor *, psd);
	xacl.info.sd_hs3->hash_type = hash_type;
	memcpy(&xacl.info.sd_hs3->hash[0], hash, XATTR_SD_HASH_SIZE);

	ndr_err = ndr_push_struct_blob(
			pblob, ctx, NULL, &xacl,
			(ndr_push_flags_fn_t)ndr_push_xattr_NTACL);

	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		DEBUG(5, ("create_acl_blob: ndr_push_xattr_NTACL failed: %s\n",
			ndr_errstr(ndr_err)));
		return ndr_map_error2ntstatus(ndr_err);;
	}

	return NT_STATUS_OK;
}

/*******************************************************************
 Pull a DATA_BLOB from an xattr given a pathname.
 If the hash doesn't match, or doesn't exist - return the underlying
 filesystem sd.
*******************************************************************/

static NTSTATUS get_nt_acl_internal(vfs_handle_struct *handle,
				files_struct *fsp,
				const char *name,
			        uint32_t security_info,
				struct security_descriptor **ppdesc)
{
	DATA_BLOB blob;
	NTSTATUS status;
	uint16_t hash_type;
	uint8_t hash[XATTR_SD_HASH_SIZE];
	uint8_t hash_tmp[XATTR_SD_HASH_SIZE];
	struct security_descriptor *psd = NULL;
	struct security_descriptor *pdesc_next = NULL;

	if (fsp && name == NULL) {
		name = fsp->fsp_name->base_name;
	}

	DEBUG(10, ("get_nt_acl_internal: name=%s\n", name));

	/* Get the full underlying sd for the hash
	   or to return as backup. */
	if (fsp) {
		status = SMB_VFS_NEXT_FGET_NT_ACL(handle,
				fsp,
				HASH_SECURITY_INFO,
				&pdesc_next);
	} else {
		status = SMB_VFS_NEXT_GET_NT_ACL(handle,
				name,
				HASH_SECURITY_INFO,
				&pdesc_next);
	}

	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(10, ("get_nt_acl_internal: get_next_acl for file %s "
			"returned %s\n",
			name,
			nt_errstr(status)));
		return status;
	}

	status = get_acl_blob(talloc_tos(), handle, fsp, name, &blob);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(10, ("get_nt_acl_internal: get_acl_blob returned %s\n",
			nt_errstr(status)));
		psd = pdesc_next;
		goto out;
	}

	status = parse_acl_blob(&blob, &psd,
				&hash_type, &hash[0]);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(10, ("parse_acl_blob returned %s\n",
				nt_errstr(status)));
		psd = pdesc_next;
		goto out;
	}

	/* Ensure the hash type is one we know. */
	switch (hash_type) {
		case XATTR_SD_HASH_TYPE_NONE:
			/* No hash, just return blob sd. */
			goto out;
		case XATTR_SD_HASH_TYPE_SHA256:
			break;
		default:
			DEBUG(10, ("get_nt_acl_internal: ACL blob revision "
				"mismatch (%u) for file %s\n",
				(unsigned int)hash_type,
				name));
			TALLOC_FREE(psd);
			psd = pdesc_next;
			goto out;
	}


	status = hash_sd_sha256(pdesc_next, hash_tmp);
	if (!NT_STATUS_IS_OK(status)) {
		TALLOC_FREE(psd);
		psd = pdesc_next;
		goto out;
	}

	if (memcmp(&hash[0], &hash_tmp[0], XATTR_SD_HASH_SIZE) == 0) {
		/* Hash matches, return blob sd. */
		goto out;
	}

	/* Hash doesn't match, return underlying sd. */
	TALLOC_FREE(psd);
	psd = pdesc_next;

  out:

	if (psd != pdesc_next) {
		/* We're returning the blob, throw
 		 * away the filesystem SD. */
		TALLOC_FREE(pdesc_next);
	}

	if (!(security_info & OWNER_SECURITY_INFORMATION)) {
		psd->owner_sid = NULL;
	}
	if (!(security_info & GROUP_SECURITY_INFORMATION)) {
		psd->group_sid = NULL;
	}
	if (!(security_info & DACL_SECURITY_INFORMATION)) {
		psd->dacl = NULL;
	}
	if (!(security_info & SACL_SECURITY_INFORMATION)) {
		psd->sacl = NULL;
	}

	TALLOC_FREE(blob.data);
	*ppdesc = psd;
	return NT_STATUS_OK;
}

/*********************************************************************
 Create a default ACL by inheriting from the parent. If no inheritance
 from the parent available, don't set anything. This will leave the actual
 permissions the new file or directory already got from the filesystem
 as the NT ACL when read.
*********************************************************************/

static NTSTATUS inherit_new_acl(vfs_handle_struct *handle,
					files_struct *fsp,
					struct security_descriptor *parent_desc,
					bool is_directory)
{
	TALLOC_CTX *ctx = talloc_tos();
	NTSTATUS status = NT_STATUS_OK;
	struct security_descriptor *psd = NULL;
	size_t size;

	if (!sd_has_inheritable_components(parent_desc, is_directory)) {
		return NT_STATUS_OK;
	}

	/* Create an inherited descriptor from the parent. */

	if (DEBUGLEVEL >= 10) {
		DEBUG(10,("inherit_new_acl: parent acl for %s is:\n",
			fsp_str_dbg(fsp) ));
		NDR_PRINT_DEBUG(security_descriptor, parent_desc);
	}

	status = se_create_child_secdesc(ctx,
			&psd,
			&size,
			parent_desc,
			&handle->conn->server_info->ptok->user_sids[PRIMARY_USER_SID_INDEX],
			&handle->conn->server_info->ptok->user_sids[PRIMARY_GROUP_SID_INDEX],
			is_directory);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		DEBUG(10,("inherit_new_acl: child acl for %s is:\n",
			fsp_str_dbg(fsp) ));
		NDR_PRINT_DEBUG(security_descriptor, parent_desc);
	}

	return SMB_VFS_FSET_NT_ACL(fsp,
				(OWNER_SECURITY_INFORMATION |
				 GROUP_SECURITY_INFORMATION |
				 DACL_SECURITY_INFORMATION),
				psd);
}

static NTSTATUS check_parent_acl_common(vfs_handle_struct *handle,
				const char *path,
				uint32_t access_mask,
				struct security_descriptor **pp_parent_desc)
{
	char *parent_name = NULL;
	struct security_descriptor *parent_desc = NULL;
	uint32_t access_granted = 0;
	NTSTATUS status;

	if (!parent_dirname(talloc_tos(), path, &parent_name, NULL)) {
		return NT_STATUS_NO_MEMORY;
	}

	status = get_nt_acl_internal(handle,
					NULL,
					parent_name,
					(OWNER_SECURITY_INFORMATION |
					 GROUP_SECURITY_INFORMATION |
					 DACL_SECURITY_INFORMATION),
					&parent_desc);

	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(10,("check_parent_acl_common: get_nt_acl_internal "
			"on directory %s for "
			"path %s returned %s\n",
			parent_name,
			path,
			nt_errstr(status) ));
		return status;
	}
	status = smb1_file_se_access_check(parent_desc,
					handle->conn->server_info->ptok,
					access_mask,
					&access_granted);
	if(!NT_STATUS_IS_OK(status)) {
		DEBUG(10,("check_parent_acl_common: access check "
			"on directory %s for "
			"path %s for mask 0x%x returned %s\n",
			parent_name,
			path,
			access_mask,
			nt_errstr(status) ));
		return status;
	}
	if (pp_parent_desc) {
		*pp_parent_desc = parent_desc;
	}
	return NT_STATUS_OK;
}

static void free_sd_common(void **ptr)
{
	TALLOC_FREE(*ptr);
}

/*********************************************************************
 Check ACL on open. For new files inherit from parent directory.
*********************************************************************/

static int open_acl_common(vfs_handle_struct *handle,
			struct smb_filename *smb_fname,
			files_struct *fsp,
			int flags,
			mode_t mode)
{
	uint32_t access_granted = 0;
	struct security_descriptor *pdesc = NULL;
	struct security_descriptor *parent_desc = NULL;
	bool file_existed = true;
	char *fname = NULL;
	NTSTATUS status;

	if (fsp->base_fsp) {
		/* Stream open. Base filename open already did the ACL check. */
		DEBUG(10,("open_acl_common: stream open on %s\n",
			fsp_str_dbg(fsp) ));
		return SMB_VFS_NEXT_OPEN(handle, smb_fname, fsp, flags, mode);
	}

	status = get_full_smb_filename(talloc_tos(), smb_fname,
				       &fname);
	if (!NT_STATUS_IS_OK(status)) {
		goto err;
	}

	status = get_nt_acl_internal(handle,
				NULL,
				fname,
				(OWNER_SECURITY_INFORMATION |
				 GROUP_SECURITY_INFORMATION |
				 DACL_SECURITY_INFORMATION),
				&pdesc);
        if (NT_STATUS_IS_OK(status)) {
		/* See if we can access it. */
		status = smb1_file_se_access_check(pdesc,
					handle->conn->server_info->ptok,
					fsp->access_mask,
					&access_granted);
		if (!NT_STATUS_IS_OK(status)) {
			DEBUG(10,("open_acl_xattr: %s open "
				"refused with error %s\n",
				fsp_str_dbg(fsp),
				nt_errstr(status) ));
			goto err;
		}
        } else if (NT_STATUS_EQUAL(status,NT_STATUS_OBJECT_NAME_NOT_FOUND)) {
		file_existed = false;
		/*
		 * If O_CREAT is true then we're trying to create a file.
		 * Check the parent directory ACL will allow this.
		 */
		if (flags & O_CREAT) {
			struct security_descriptor *psd = NULL;

			status = check_parent_acl_common(handle, fname,
					SEC_DIR_ADD_FILE, &parent_desc);
			if (!NT_STATUS_IS_OK(status)) {
				goto err;
			}
			/* Cache the parent security descriptor for
			 * later use. We do have an fsp here, but to
			 * keep the code consistent with the directory
			 * case which doesn't, use the handle. */

			/* Attach this to the conn, move from talloc_tos(). */
			psd = (struct security_descriptor *)talloc_move(handle->conn,
				&parent_desc);

			if (!psd) {
				status = NT_STATUS_NO_MEMORY;
				goto err;
			}
			status = NT_STATUS_NO_MEMORY;
			SMB_VFS_HANDLE_SET_DATA(handle, psd, free_sd_common,
				struct security_descriptor *, goto err);
			status = NT_STATUS_OK;
		}
	}

	DEBUG(10,("open_acl_xattr: get_nt_acl_attr_internal for "
		"%s returned %s\n",
		fsp_str_dbg(fsp),
		nt_errstr(status) ));

	fsp->fh->fd = SMB_VFS_NEXT_OPEN(handle, smb_fname, fsp, flags, mode);
	return fsp->fh->fd;

  err:

	errno = map_errno_from_nt_status(status);
	return -1;
}

static int mkdir_acl_common(vfs_handle_struct *handle, const char *path, mode_t mode)
{
	int ret;
	NTSTATUS status;
	SMB_STRUCT_STAT sbuf;

	ret = vfs_stat_smb_fname(handle->conn, path, &sbuf);
	if (ret == -1 && errno == ENOENT) {
		struct security_descriptor *parent_desc = NULL;
		struct security_descriptor *psd = NULL;

		/* We're creating a new directory. */
		status = check_parent_acl_common(handle, path,
				SEC_DIR_ADD_SUBDIR, &parent_desc);
		if (!NT_STATUS_IS_OK(status)) {
			errno = map_errno_from_nt_status(status);
			return -1;
		}

		/* Cache the parent security descriptor for
		 * later use. We don't have an fsp here so
		 * use the handle. */

		/* Attach this to the conn, move from talloc_tos(). */
		psd = (struct security_descriptor *)talloc_move(handle->conn,
				&parent_desc);

		if (!psd) {
			return -1;
		}
		SMB_VFS_HANDLE_SET_DATA(handle, psd, free_sd_common,
			struct security_descriptor *, return -1);
	}

	return SMB_VFS_NEXT_MKDIR(handle, path, mode);
}

/*********************************************************************
 Fetch a security descriptor given an fsp.
*********************************************************************/

static NTSTATUS fget_nt_acl_common(vfs_handle_struct *handle, files_struct *fsp,
        uint32_t security_info, struct security_descriptor **ppdesc)
{
	return get_nt_acl_internal(handle, fsp,
				NULL, security_info, ppdesc);
}

/*********************************************************************
 Fetch a security descriptor given a pathname.
*********************************************************************/

static NTSTATUS get_nt_acl_common(vfs_handle_struct *handle,
        const char *name, uint32_t security_info, struct security_descriptor **ppdesc)
{
	return get_nt_acl_internal(handle, NULL,
				name, security_info, ppdesc);
}

/*********************************************************************
 Store a security descriptor given an fsp.
*********************************************************************/

static NTSTATUS fset_nt_acl_common(vfs_handle_struct *handle, files_struct *fsp,
        uint32_t security_info_sent, const struct security_descriptor *psd)
{
	NTSTATUS status;
	DATA_BLOB blob;
	struct security_descriptor *pdesc_next = NULL;
	uint8_t hash[XATTR_SD_HASH_SIZE];

	if (DEBUGLEVEL >= 10) {
		DEBUG(10,("fset_nt_acl_xattr: incoming sd for file %s\n",
			  fsp_str_dbg(fsp)));
		NDR_PRINT_DEBUG(security_descriptor,
			CONST_DISCARD(struct security_descriptor *,psd));
	}

        /* Ensure we have OWNER/GROUP/DACL set. */

	if ((security_info_sent & (OWNER_SECURITY_INFORMATION|
				GROUP_SECURITY_INFORMATION|
				DACL_SECURITY_INFORMATION)) !=
				(OWNER_SECURITY_INFORMATION|
				 GROUP_SECURITY_INFORMATION|
				 DACL_SECURITY_INFORMATION)) {
		/* No we don't - read from the existing SD. */
		struct security_descriptor *nc_psd = NULL;

		status = get_nt_acl_internal(handle, fsp,
				NULL,
				(OWNER_SECURITY_INFORMATION|
				 GROUP_SECURITY_INFORMATION|
				 DACL_SECURITY_INFORMATION),
				&nc_psd);

		if (!NT_STATUS_IS_OK(status)) {
			return status;
		}

		/* This is safe as nc_psd is discarded at fn exit. */
		if (security_info_sent & OWNER_SECURITY_INFORMATION) {
			nc_psd->owner_sid = psd->owner_sid;
		}
		security_info_sent |= OWNER_SECURITY_INFORMATION;

		if (security_info_sent & GROUP_SECURITY_INFORMATION) {
			nc_psd->group_sid = psd->group_sid;
		}
		security_info_sent |= GROUP_SECURITY_INFORMATION;

		if (security_info_sent & DACL_SECURITY_INFORMATION) {
			nc_psd->dacl = dup_sec_acl(talloc_tos(), psd->dacl);
			if (nc_psd->dacl == NULL) {
				return NT_STATUS_NO_MEMORY;
			}
		}
		security_info_sent |= DACL_SECURITY_INFORMATION;
		psd = nc_psd;
	}

	status = SMB_VFS_NEXT_FSET_NT_ACL(handle, fsp, security_info_sent, psd);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	/* Get the full underlying sd, then hash. */
	status = SMB_VFS_NEXT_FGET_NT_ACL(handle,
				fsp,
				HASH_SECURITY_INFO,
				&pdesc_next);

	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	status = hash_sd_sha256(pdesc_next, hash);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (DEBUGLEVEL >= 10) {
		DEBUG(10,("fset_nt_acl_xattr: storing xattr sd for file %s\n",
			  fsp_str_dbg(fsp)));
		NDR_PRINT_DEBUG(security_descriptor,
			CONST_DISCARD(struct security_descriptor *,psd));
	}
	create_acl_blob(psd, &blob, XATTR_SD_HASH_TYPE_SHA256, hash);
	store_acl_blob_fsp(handle, fsp, &blob);

	return NT_STATUS_OK;
}

static SMB_STRUCT_DIR *opendir_acl_common(vfs_handle_struct *handle,
			const char *fname, const char *mask, uint32 attr)
{
	NTSTATUS status = check_parent_acl_common(handle, fname,
					SEC_DIR_LIST, NULL);

	if (!NT_STATUS_IS_OK(status)) {
		errno = map_errno_from_nt_status(status);
		return NULL;
	}
	return SMB_VFS_NEXT_OPENDIR(handle, fname, mask, attr);
}

static NTSTATUS create_file_acl_common(struct vfs_handle_struct *handle,
				struct smb_request *req,
				uint16_t root_dir_fid,
				struct smb_filename *smb_fname,
				uint32_t access_mask,
				uint32_t share_access,
				uint32_t create_disposition,
				uint32_t create_options,
				uint32_t file_attributes,
				uint32_t oplock_request,
				uint64_t allocation_size,
				struct security_descriptor *sd,
				struct ea_list *ea_list,
				files_struct **result,
				int *pinfo)
{
	NTSTATUS status, status1;
	files_struct *fsp = NULL;
	int info;
	struct security_descriptor *parent_sd = NULL;

	status = SMB_VFS_NEXT_CREATE_FILE(handle,
					req,
					root_dir_fid,
					smb_fname,
					access_mask,
					share_access,
					create_disposition,
					create_options,
					file_attributes,
					oplock_request,
					allocation_size,
					sd,
					ea_list,
					result,
					&info);

	if (info != FILE_WAS_CREATED) {
		/* File/directory was opened, not created. */
		goto out;
	}

	fsp = *result;

	if (!NT_STATUS_IS_OK(status) || fsp == NULL) {
		/* Only handle success. */
		goto out;
	}

	if (sd) {
		/* Security descriptor already set. */
		goto out;
	}

	if (fsp->base_fsp) {
		/* Stream open. */
		goto out;
	}


	/* We must have a cached parent sd in this case.
	 * attached to the handle. */

	SMB_VFS_HANDLE_GET_DATA(handle, parent_sd,
		struct security_descriptor,
		goto err);

	if (!parent_sd) {
		goto err;
	}

	/* New directory - inherit from parent. */
	status1 = inherit_new_acl(handle, fsp, parent_sd, fsp->is_directory);

	if (!NT_STATUS_IS_OK(status1)) {
		DEBUG(1,("create_file_acl_common: error setting "
			"sd for %s (%s)\n",
			fsp_str_dbg(fsp),
			nt_errstr(status1) ));
	}

  out:

	/* Ensure we never leave attached data around. */
	SMB_VFS_HANDLE_FREE_DATA(handle);

	if (NT_STATUS_IS_OK(status) && pinfo) {
		*pinfo = info;
	}
	return status;

  err:

	smb_panic("create_file_acl_common: logic error.\n");
	/* NOTREACHED */
	return status;
}