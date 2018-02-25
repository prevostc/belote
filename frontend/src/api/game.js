import gql from "graphql-tag";

export const playerFragment = gql`
    fragment playerFragment on Player {
        uuid
        team
        spot
        name
        isDealer
        actionNeeded
    }
`;


export const gameFragment = gql`
    fragment gameFragment on Game {
        uuid
        phase
        table {
            color
            motif
        }
        players {
            ...playerFragment
        }
        contract {
            value
            player {
                ...playerFragment
            }
        }
    }
    
    ${playerFragment}
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
            ...playerFragment
            game {
                ...gameFragment
            }
        }
    }
    ${gameFragment}
    ${playerFragment}
`;

// write the newly created project to the query cache
// http://dev.apollodata.com/core/read-and-write.html#updating-the-cache-after-a-mutation
export const joinGameUpdate = (variables) => (proxy, { data: { joinGame } }) => {
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
                game: newGame
            };
        }
    });
};