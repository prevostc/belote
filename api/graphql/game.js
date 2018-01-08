const NodeCache = require( "node-cache" )
const uuid = require('uuid/v4');
const util = require('util');

const gameCache = new NodeCache( { stdTTL: 24 * 60 * 60, checkperiod: 60 } );

const schema = `
    type Game {
        uuid: String!
    }
    
    type Query {
        game(uuid: String!): Game!
    }
    
    type Mutation {
        createGame(name: String): Game!
    }
`

const resolvers = {
    Query: {
        game: async(ctx, args) => {
            const game = await util.promisify(gameCache.get)(args.uuid);
            if (! game) {
                throw new Error('Game not found')
            }
            return game
        },
    },
    Mutation: {
        createGame: async() => {
            const game = {'uuid': uuid() }
            await util.promisify(gameCache.set)(game.uuid, game);
            return game
        },
    },
}

module.exports = { schema, resolvers }
