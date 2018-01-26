#!/bin/bash
awk 'BEGIN {FS="[ =\"]+"; OFS="\t"} /retention_time_sec/{rt=$15} /hit_rank/{print $5,rt}' $1 > $2
