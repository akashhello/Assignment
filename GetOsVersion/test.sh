#!/bin/bash


var="129 148     181"
vars=( $var )
echo "First  word of var: '${vars[0]}'"
echo "Second word of var: '${vars[1]}'"
echo "Third  word of var: '${vars[2]}'"
echo "Number of words in var: '${#vars[@]}'"
