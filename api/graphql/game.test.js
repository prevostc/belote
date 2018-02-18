const request = require('supertest');
const app = require('../app');
const store = require('../store/gameStore');
const { engine, format } = require('../engine');
const SPOT = format.consts.SPOT;

function graphqlQuery({ query, variables }) {
    const req = request(app)
        .post('/graphql')
        .set('Accept', 'application/json')
        .send({ query, variables })
    return req
}

function createGameStub({ uuid, players = [] }) {
    let game = engine.createGame(uuid);
    game = players.reduce((game, { uuid: playerUuid, spot, name }) => {
        return engine.raiseErrorOrUnboxState(engine.joinGame(playerUuid, name, spot, game));
    }, game);
    return store.save(game);
}


describe('graphql game api', () => {

    it('should create a game', async() => {
        const response = await graphqlQuery({
            query: `
                mutation createGame($name: String!) {
                    createGame(name: $name) {
                       uuid
                       players {
                         uuid
                       }
                    }
                }
            `,
            variables: {
                name: "TEST",
            },
        });
        expect(response.body.errors).not.toBeDefined();
        const gameData = response.body.data.createGame;
        // should have a uuid
        expect(gameData.uuid.length).toEqual(36)

        // should not have players yet
        expect(gameData.players).toEqual([])
    })

    it('should fetch a stored game', async() => {
        await createGameStub({
            uuid: 'abc',
            players: [
                {uuid: '1', spot: SPOT.NORTH, name: 'a' },
                {uuid: '2', spot: SPOT.SOUTH, name: 'b' },
                {uuid: '3', spot: SPOT.EAST, name: 'c' },
                {uuid: '4', spot: SPOT.WEST, name: 'd' },
            ]
        });
        const response = await graphqlQuery({
            query: `{
                game(uuid: "abc") {
                    uuid
                    players {
                        uuid
                        spot
                        isDealer
                        cards {
                            color
                            motif
                        }
                    }
                    phase
                }
            }`,
        });
        expect(response.body.errors).not.toBeDefined();
        const gameData = response.body.data.game;
        expect(gameData.uuid).toEqual('abc');
        expect(gameData.players.length).toEqual(4);
        const northPlayer = gameData.players.find(p => p.spot === "NORTH")
        expect(northPlayer.isDealer).toEqual(true);
        expect(northPlayer.spot).toEqual("NORTH");
        expect(northPlayer.cards.length).toEqual(8);
    });

    it('should join a game', async() => {
        await createGameStub({uuid: 'abc'});
        const response = await graphqlQuery({
            query: `
                mutation joinGame($gameUuid: String!, $playerName: String!, $spot: PlayerSpot!) {
                    joinGame(gameUuid: $gameUuid, playerName: $playerName, spot: $spot) {
                        uuid
                        name
                        spot
                        game {
                            uuid
                            players {
                                uuid
                                isDealer
                                cards {
                                    motif
                                    color
                                }
                            }
                            phase
                       }
                    }
                }`,
            variables: {
                gameUuid: 'abc',
                playerName: 'Mitch',
                spot: "EAST",
            }
        });
        expect(response.body.errors).not.toBeDefined();
        expect(response.body.data.joinGame.name).toEqual('Mitch');
        expect(response.body.data.joinGame.spot).toEqual('EAST');
        expect(response.body.data.joinGame.uuid.length).toEqual(36);
        expect(response.body.data.joinGame.game.uuid).toEqual('abc');
        expect(response.body.data.joinGame.game.phase).toEqual('INITIAL');
        expect(response.body.data.joinGame.game.players.length).toEqual(1);
    });
})
