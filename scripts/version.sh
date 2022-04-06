#!/bin/sh

if [ ! -d  "version.toml" ]; then
  echo "#Versioning (Auto-generated)" > version.toml
fi

V_HASH=$(git rev-parse HEAD | cut -c1-10);
V_BRANCH=$(git rev-parse --abbrev-ref HEAD);

echo hash=\"$V_HASH\" >> version.toml
echo branch=\"$V_BRANCH\" >> version.toml
