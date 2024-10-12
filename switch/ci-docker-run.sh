#!/bin/bash
docker run --rm -v "$(pwd)":/src -w /src sergiou87/switch-dev-triplane-turmoil "$@"