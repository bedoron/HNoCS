#! /bin/bash
RES_SUFF="`date +%F_%s`"
RES_DIR="${HOME}/Backup/backup_${RES_SUFF}"
echo "Creating ${RES_DIR}"
tar --exclude='simulations' --exclude='out' -cjf $RES_DIR.tar.bz2 *


