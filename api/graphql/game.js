const uuid = require('uuid/v4');
const { PubSub } = require('graphql-subscriptions');
const engine = require('../engine');
const gameStore = require('../store');

const pubsub = new PubSub();

const schema = `
    type Game {
        uuid: String!
        jsonState: String!
        players: [Player]!
    }
    
    type Player {
        uuid: String!
        name: String!
    }
    
    type Query {
        game(uuid: String!): Game!
    }
    
    type Mutation {
        createGame(name: String): Game!
        joinGame(gameUuid: String!, name: String!): Game!
    }
    
    type Subscription {
        gameStateChanged(uuid: String!): Game!
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
            const game = await gameStore.get(args.uuid);
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
            await gameStore.set(u, game);
            return game
        },
    },
    Subscription: {
        gameStateChanged: {
            subscribe: () => pubsub.asyncIterator('gameStateChanged')
        }
    },
}

module.exports = { schema, resolvers }
