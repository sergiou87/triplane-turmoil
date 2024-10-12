#!/bin/bash
docker run --mount type=bind,source=${PWD},target=/src --rm -it sergiou87/switch-dev-triplane-turmoil "$@"
