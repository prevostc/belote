const express = require('express')
const { graphqlExpress } = require('graphql-server-express')
const bodyParser = require('body-parser')
const schema = require('./graphql')
const cors = require('cors')
const { addErrorLoggingToSchema } = require('graphql-tools')
const logger = require('./logger')
const { FRONT_URL } = require('./config');
const app = express()

const corsMdl = cors({ origin: FRONT_URL });
app.use('*', corsMdl);
app.options('*', corsMdl);

// to support JSON-encoded bodies
app.use(bodyParser.json())
addErrorLoggingToSchema(schema, logger)
app.use('/graphql', graphqlExpress({ schema }))

module.exports = app
