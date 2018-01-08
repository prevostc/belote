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
        createGame(name: String!): Game!
    }
`

const resolvers = {
    Query: {
        game: async(ctx, args) => {
            const game = await util.promisify(gameCache.get(args.uuid));
            if (! game) {
                throw new Error('Game not found')
            }
            return game
        },
    },
    Mutation: {
        createGame: async() => {
            const u = uuid();
            return util.promisify(gameCache.set(u, {'uuid': u }));
        },
    },
}

module.exports = { schema, resolvers }
