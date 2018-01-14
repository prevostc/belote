const NodeCache = require( "node-cache" )
const uuid = require('uuid/v4');
const util = require('util');
const engine = require('../engine')
const gameCache = new NodeCache( { stdTTL: 24 * 60 * 60, checkperiod: 60 } );

const schema = `
    enum Color {
        Spades
        Hearts 
        Diamonds 
        Clubs
    }
    
    type Mutation {
        makeBid(color: Color!, value: Int!): Game!
    }
`

const resolvers = {
    Mutation: {
        makeBid: async() => {
            const game = engine.makeBid(u);
            return game
        },
    },
}

module.exports = { schema, resolvers }
