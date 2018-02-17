import gql from "graphql-tag";

export const gameFragment = gql`
    fragment gameFragment on Game {
        uuid
        gameState
        phase
        players {
            uuid
            spot
            name
            
            isDealer
        }
    }
`;

export const gameQuery = gql`
    query game($uuid: String!) {
        game(uuid: $uuid) {
            ...gameFragment
        }
    }

    ${gameFragment}
`;

export const joinGameQuery = gql`
    mutation joinGame($uuid: String!, $playerUuid: String, $name: String!, $spot: PlayerSpot!) {
        joinGame(gameUuid: $uuid, playerUuid: $playerUuid, playerName: $name, spot: $spot) {
            uuid
            name
            spot
            game {
                ...gameFragment
            }
        }
    }
    ${gameFragment}
`;

// write the newly created project to the query cache
// http://dev.apollodata.com/core/read-and-write.html#updating-the-cache-after-a-mutation
export const joinGameUpdate = ({ uuid }) => (proxy, { data: { joinGame } }) => {
    const variables = { uuid };
    const query = gameQuery
    const data = proxy.readQuery({ query, variables })
    data.game = joinGame.game;
    proxy.writeQuery({ query, variables, data })
};

const gameChangeSubscription = gql`
    subscription gameStateChanged($uuid: String!) {
      gameStateChanged(uuid: $uuid) {
        ...gameFragment
      }
    }
    ${gameFragment}
`;

export const subscribeToChange = (query) => ({uuid}) => {
    return query.subscribeToMore({
        document: gameChangeSubscription,
        variables: { uuid },
        updateQuery: (prev, {subscriptionData}) => {
            if (!subscriptionData.data) {
                return prev;
            }
            const newGame = subscriptionData.data.gameStateChanged;
            return {
                ...prev,
                game: {
                    ...prev.game,
                    gameState: newGame.gameState,
                    players: newGame.players,
                }
            };
        }
    });
};