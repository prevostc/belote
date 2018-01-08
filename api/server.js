const express = require('express')
const { graphqlExpress } = require('graphql-server-express')
const bodyParser = require('body-parser')
const schema = require('./graphql')
const cors = require('cors')
const { addErrorLoggingToSchema } = require('graphql-tools')
const logger = require('./logger')

const app = express()

app.use(cors())
app.options('*', cors()) // TODO: don't allow *

// to support JSON-encoded bodies
app.use(bodyParser.json())
addErrorLoggingToSchema(schema, logger)
app.use(
    '/graphql',
    graphqlExpress(request => ({
        schema,
        context: { user: request.user },
    }))
)

const port = process.env.PORT || 4000
const host = process.env.HOST || '0.0.0.0'
app.listen(port, host)

logger.info(`Running a GraphQL API server at http://${host}:${port}/graphql`)
