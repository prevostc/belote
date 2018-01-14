const request = require('supertest');
const app = require('../app');
const store = require('../store');

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
    })

    it('should fetch a stored game', async() => {
        const game = { uuid: 'ABC' };
        await store.set('abc', game);
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

})
