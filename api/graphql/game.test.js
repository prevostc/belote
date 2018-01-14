const request = require('supertest')
const app = require('../app')

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
        })

        expect(response.body.data.createGame.uuid.length).toEqual(36)
    })

})
