const { execute, subscribe } = require('graphql');
const { createServer } = require('http');
const { SubscriptionServer } = require('subscriptions-transport-ws');
const logger = require('./logger');
const schema = require('./graphql');
const app = require('./app');
const { HOST, PORT } = require('./config');

// Wrap the Express server
const server = createServer(app);
server.listen(PORT, () => {
    logger.info(`Running a GraphQL API server at http://${HOST}:${PORT}/graphql`)
    logger.info(`Running a GraphQL Subscriptions server at ws://${HOST}:${PORT}/subscriptions`)
    // Set up the WebSocket for handling GraphQL subscriptions
    new SubscriptionServer({
        execute,
        subscribe,
        schema
    }, {
        server: server,
        path: '/subscriptions',
    });
});