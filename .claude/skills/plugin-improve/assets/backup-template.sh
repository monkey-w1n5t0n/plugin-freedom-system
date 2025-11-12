#!/bin/bash
# Backup plugin source to timestamped directory

PLUGIN_NAME="$1"
CURRENT_VERSION="$2"

mkdir -p "backups/${PLUGIN_NAME}/v${CURRENT_VERSION}/"
cp -r "plugins/${PLUGIN_NAME}/" "backups/${PLUGIN_NAME}/v${CURRENT_VERSION}/"

echo "✓ Backup created: backups/${PLUGIN_NAME}/v${CURRENT_VERSION}/"
