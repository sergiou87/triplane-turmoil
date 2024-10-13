#!/bin/bash
MSYS_NO_PATHCONV=1 docker run --rm -ti -v "$(pwd)":/src -w /src sergiou87/windows-dev-triplane-turmoil "$@"
