const NodeCache = require( "node-cache" )
const uuid = require('uuid/v4');
const util = require('util');
const engine = require('../engine')
const gameCache = new NodeCache( { stdTTL: 24 * 60 * 60, checkperiod: 60 } );

const schema = `
    type Game {
        uuid: String!
        jsonState: String!
    }
    
    type Query {
        game(uuid: String!): Game!
    }
    
    type Mutation {
        createGame(name: String): Game!
    }
`

const resolvers = {
    Game: {
        jsonState: (game) => {
            return JSON.stringify(game.state)
        }
    },
    Query: {
        game: async (ctx, args) => {
            const game = await util.promisify(gameCache.get)(args.uuid);
            if (! game) {
                throw new Error('Game not found')
            }
            return game
        },
    },
    Mutation: {
        createGame: async() => {
            const u = uuid();
            const game = engine.createGame(u);
            await util.promisify(gameCache.set)(u, game);
            return game
        },
    },
}

module.exports = { schema, resolvers }
