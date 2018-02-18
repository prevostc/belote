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
        await createGameStub({
            uuid: 'abc',
            players: [
                {uuid: '1', spot: SPOT.NORTH, name: 'a' },
                {uuid: '2', spot: SPOT.SOUTH, name: 'b' },
            ]
        });
        const response = await graphqlQuery({
            query: `
                mutation joinGame($gameUuid: String!, $playerUuid: String, $playerName: String!, $spot: PlayerSpot!) {
                    joinGame(gameUuid: $gameUuid, playerUuid: $playerUuid, playerName: $playerName, spot: $spot) {
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
                playerUuid: null,
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
        expect(response.body.data.joinGame.game.players.length).toEqual(3);
    });


    it('should change spot', async() => {
        await createGameStub({
            uuid: 'abc',
            players: [
                {uuid: '1', spot: SPOT.NORTH, name: 'a' },
                {uuid: '2', spot: SPOT.SOUTH, name: 'b' },
            ]
        });
        const response = await graphqlQuery({
            query: `
                mutation joinGame($gameUuid: String!, $playerUuid: String, $playerName: String!, $spot: PlayerSpot!) {
                    joinGame(gameUuid: $gameUuid, playerUuid: $playerUuid, playerName: $playerName, spot: $spot) {
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
                playerUuid: "1",
                playerName: 'a',
                spot: "EAST",
            }
        });
        expect(response.body.errors).not.toBeDefined();
        expect(response.body.data.joinGame.name).toEqual('a');
        expect(response.body.data.joinGame.spot).toEqual('EAST');
        expect(response.body.data.joinGame.uuid).toEqual("1");
        expect(response.body.data.joinGame.game.uuid).toEqual('abc');
        expect(response.body.data.joinGame.game.phase).toEqual('INITIAL');
        expect(response.body.data.joinGame.game.players.length).toEqual(2);
    });


    it('should make a bid', async() => {
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
            query: `
                mutation bid($gameUuid: String!, $playerUuid: String!, $value: Int!, $color: CardColor!) {
                    bid(gameUuid: $gameUuid, playerUuid: $playerUuid, value: $value, color: $color) {
                        uuid
                        bids {
                            isPass
                            value
                            color
                            player {
                                uuid
                            }
                        }
                    }
                }`,
            variables: {
                gameUuid: 'abc',
                playerUuid: '1',
                value: 80,
                color: "HEARTS",
            }
        });
        expect(response.body.errors).not.toBeDefined();
        const game = response.body.data.bid;
        expect(game.uuid).toEqual('abc');
        expect(game.bids.length).toEqual(1);
        const bid = game.bids[0];
        expect(bid.isPass).toEqual(false);
        expect(bid.value).toEqual(80);
        expect(bid.color).toEqual("HEARTS");
        const player = bid.player;
        expect(player.uuid).toEqual("1");
    });

    it('should be able to pass', async() => {
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
            query: `
                mutation pass($gameUuid: String!, $playerUuid: String!) {
                    pass(gameUuid: $gameUuid, playerUuid: $playerUuid) {
                        uuid
                        bids {
                            isPass
                            value
                            color
                            player {
                                uuid
                            }
                        }
                    }
                }`,
            variables: {
                gameUuid: 'abc',
                playerUuid: '1'
            }
        });
        expect(response.body.errors).not.toBeDefined();
        const game = response.body.data.pass;
        expect(game.uuid).toEqual('abc');
        expect(game.bids.length).toEqual(1);
        const bid = game.bids[0];
        expect(bid.isPass).toEqual(true);
        expect(bid.value).toEqual(null);
        expect(bid.color).toEqual(null);
        const player = bid.player;
        expect(player.uuid).toEqual("1");
    });
})
