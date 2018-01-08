const { makeExecutableSchema } = require('graphql-tools')
const { mergeTypes } = require('merge-graphql-schemas')
const merge = require('lodash/merge')

const game = require('./game')

const typeDefs = mergeTypes([
    game.schema,
])

const resolvers = merge(
    game.resolvers,
)

const schema = makeExecutableSchema({
    typeDefs,
    resolvers,
})

module.exports = schema
