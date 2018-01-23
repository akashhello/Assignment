$SystemInfo = @()

$DiskList = GET-WMIOBJECT –query “SELECT * from win32_logicaldisk WHERE DriveType = 3” –computername “LAPTOP-Q2RR0J57”
#$DiskList|Sort-Object @{e={([int]$_.TotalSpace)}}
$DiskList = $DiskList|Sort-Object -Property @{Expression={[int]($_.Size/1GB) - [int]($_.FreeSpace/1GB)}}

foreach($Disk in $DiskList)
{
    
    $FreeSpace = "{0:N2}" -f ($Disk.FreeSpace / 1GB)
	$TotalSpace = "{0:N2}" -f ($Disk.Size / 1GB) 
    $UsedSpace = [int]$TotalSpace - [int]$FreeSpace;
    #Write-Output "output" | $UsedSpace $TotalSpace $FreeSpace $Disk.Name
    Write-Output "UsedSpace:" $UsedSpace "Totalspace:" $TotalSpace "Name:"  $Disk.Name | Format-Table -Auto  
}
