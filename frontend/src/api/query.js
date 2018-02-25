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


export const bidFragment = gql`
    fragment bidFragment on Bid {
        isPass
        value
        color
        player {
            ...playerFragment
        }
    }
    
    ${playerFragment}
`;

export const cardFragment = gql`
    fragment cardFragment on Card {
        color
        motif
        canBePlayed(gameUuid: $gameUuid, playerUuid: $playerUuid)
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
            trump
            player {
                ...playerFragment
            }
        }
        cards (playerUuid: $playerUuid) {
            ...cardFragment
        }
        bids {
            ...bidFragment
        }
    }
    
    ${playerFragment}
    ${bidFragment}
    ${cardFragment}
`;

export const gameQuery = gql`
    query game($gameUuid: ID!, $playerUuid: ID!) {
        game(uuid: $gameUuid) {
            ...gameFragment
        }
    }

    ${gameFragment}
`;



// write the newly created project to the query cache
// http://dev.apollodata.com/core/read-and-write.html#updating-the-cache-after-a-mutation
const createUpdateFn = (queryName) => (variables) => (proxy, { data }) => {
    const queryContent = proxy.readQuery({ query: gameQuery, variables })
    queryContent.game = data[queryName];
    proxy.writeQuery({ query: gameQuery, variables, data: queryContent })
};

export const joinGameMutationUpdate = createUpdateFn('joinGame');
export const joinGameMutation = gql`
    mutation joinGame($gameUuid: ID!, $playerUuid: ID, $name: String!, $spot: PlayerSpot!) {
        joinGame(gameUuid: $gameUuid, playerUuid: $playerUuid, playerName: $name, spot: $spot) {
            ...playerFragment
        }
    }
    ${playerFragment}
`;

export const playCardMutationUpdate = createUpdateFn('playCard')
export const playCardMutation = gql`
    mutation playCard($gameUuid: ID!, $playerUuid: ID!, $color: CardColor!, $motif: CardMotif!) {
        playCard(gameUuid: $gameUuid, playerUuid: $playerUuid, color: $color, motif: $motif) {
            ...gameFragment
        }
    }
    ${gameFragment}
`;


export const bidMutationUpdate = createUpdateFn('bid');
export const bidMutation = gql`
    mutation bid($gameUuid: ID!, $playerUuid: ID!, $value: Int!, $color: CardColor!) {
        bid(gameUuid: $gameUuid, playerUuid: $playerUuid, value: $value, color: $color) {
            ...gameFragment
        }
    }
    ${gameFragment}
`;
export const passMutationUpdate = createUpdateFn('pass');
export const passMutation = gql`
    mutation pass($gameUuid: ID!, $playerUuid: ID!) {
        pass(gameUuid: $gameUuid, playerUuid: $playerUuid) {
            ...gameFragment
        }
    }
    ${gameFragment}
`;

export const createGameMutationUpdate = createUpdateFn('pass');
export const createGameMutation = gql`
    mutation createGame($name: String!) {
        createGame(name: $name) {
            uuid
        }
    }
`;

export const gameChangeSubscription = gql`
    subscription gameStateChanged($gameUuid: ID!, $playerUuid: ID!) {
      gameStateChanged(uuid: $gameUuid) {
        ...gameFragment
      }
    }
    ${gameFragment}
`;
