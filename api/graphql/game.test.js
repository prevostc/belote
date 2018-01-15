const request = require('supertest');
const app = require('../app');
const store = require('../store/gameStore');

function graphqlQuery({ query, variables }) {
    const req = request(app)
        .post('/graphql')
        .set('Accept', 'application/json')
        .send({ query, variables })
    return req
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
        const game = { uuid: 'abc' };
        await store.save(game);
        const response = await graphqlQuery({
            query: `{
                game(uuid: "abc") {
                   uuid
                }
            }`,
        });
        const gameData = response.body.data.game;
        // should have a uuid
        expect(gameData).toEqual(game)
    })

    it('should join a game', async() => {
        const game = { uuid: 'abc', players: [] };
        await store.save(game);
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
        expect(response.body.data.joinGame.game.players.length).toEqual(1);
    })

})
