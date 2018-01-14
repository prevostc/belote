const uuid = require('uuid/v4');
const { PubSub } = require('graphql-subscriptions');
const engine = require('../engine');
const gameStore = require('../store/gameStore');
const playerStore = require('../store/playerStore');

const pubsub = new PubSub();

const schema = `
    enum PlayerSpot {
      NORTH
      SOUTH
      EAST
      WEST
    }

    type Game {
        uuid: String!
        jsonState: String!
        players: [Player]!
    }
    
    type Player {
        uuid: String!
        name: String!
        spot: PlayerSpot!
        game: Game!
    }
    
    type Query {
        game(uuid: String!): Game!
    }
    
    type Mutation {
        createGame(name: String): Game!
        joinGame(gameUuid: String!, playerName: String!, spot: PlayerSpot!): Player!
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
    Player: {
        game: async (player) => {
            const game = await gameStore.get(player.gameUuid);
            return game;
        }
    },
    Query: {
        game: async(ctx, args) => {
            const game = await gameStore.get(args.uuid);
            return game;
        },
    },
    Mutation: {
        createGame: async() => {
            const game = engine.createGame(uuid());
            await gameStore.save(game);
            return game
        },
        joinGame: async(ctx, args) => {
            let game = await gameStore.get(args.gameUuid);
            const player = { uuid: uuid(), name: args.playerName, spot: args.spot, gameUuid: game.uuid };
            game = engine.joinGame(game, player.name, player.spot);
            await gameStore.save(game);
            await playerStore.save(player);
            return player;
        }
    },
    Subscription: {
        gameStateChanged: {
            subscribe: () => pubsub.asyncIterator('gameStateChanged')
        }
    },
}

module.exports = { schema, resolvers }
