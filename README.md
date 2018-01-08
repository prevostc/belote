# Status

[![Build Status](https://travis-ci.org/prevostc/belote.svg?branch=master)](https://travis-ci.org/prevostc/belote)

# Commands

See package.json scripts

# Deploy

    docker build -t belote .
    docker run --rm -it -p 4000:4000 -e "NODE_ENV=production" -u "node" belote