const request = require('supertest');
const app = require('../app');
const store = require('../store/gameStore');
const { engine } = require('../engine');

function graphqlQuery({ query, variables }) {
    const req = request(app)
        .post('/graphql')
        .set('Accept', 'application/json')
        .send({ query, variables })
    return req
}

function createGameStub({ uuid, players = [] }) {
    const game = engine.createGame(uuid);
    players.forEach((p) => {});
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
        const gameData = response.body.data.createGame;
        // should have a uuid
        expect(gameData.uuid.length).toEqual(36)

        // should not have players yet
        expect(gameData.players).toEqual([])
    })

    it('should fetch a stored game', async() => {
        await createGameStub({uuid: 'abc'});
        const response = await graphqlQuery({
            query: `{
                game(uuid: "abc") {
                   uuid
                }
            }`,
        });
        const gameData = response.body.data.game;
        // should have a uuid
        expect(gameData.uuid).toEqual('abc');
    })

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
        expect(response.body.data.joinGame.name).toEqual('Mitch');
        expect(response.body.data.joinGame.spot).toEqual('EAST');
        expect(response.body.data.joinGame.uuid.length).toEqual(36);
        expect(response.body.data.joinGame.game.uuid).toEqual('abc');
        expect(response.body.data.joinGame.game.phase).toEqual('INITIAL');
        expect(response.body.data.joinGame.game.players.length).toEqual(1);
    });
})
