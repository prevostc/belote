
module.exports = {
    PORT: process.env.PORT || 4000,
    HOST: process.env.HOST || '0.0.0.0',
    FRONT_URL: process.env.FRONT_URL || `http://localhost:3000`,
    API_URL: process.env.FRONT_URL || `http://localhost:4000/graphql`,
    API_WS_URL: process.env.FRONT_URL || `ws://localhost:4000/subscriptions`,
};