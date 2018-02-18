import gql from "graphql-tag";

export const bidFragment = gql`
    fragment bidFragment on Bid {
        isPass
        value
        color
        player {
            spot
        }
    }
`;

export const BidsQuery = gql`
    query game($gameUuid: String!) {
        game(uuid: $gameUuid) {
            bids {
                ...bidFragment
            }
        }
    }

    ${bidFragment}
`;

export const bidMutation = gql`
    mutation bid($gameUuid: String!, $playerUuid: String!, $value: Int!, $color: CardColor!) {
        bid(gameUuid: $gameUuid, playerUuid: $playerUuid, value: $value, color: $color) {
            bids {
                ...bidFragment
            }
        }
    }
    ${bidFragment}
`;
export const passMutation = gql`
    mutation pass($gameUuid: String!, $playerUuid: String!) {
        pass(gameUuid: $gameUuid, playerUuid: $playerUuid) {
            bids {
                ...bidFragment
            }
        }
    }
    ${bidFragment}
`;

// write the newly created project to the query cache
// http://dev.apollodata.com/core/read-and-write.html#updating-the-cache-after-a-mutation
export const bidMutationUpdate = ({ gameUuid }) => (proxy, { data: { bid } }) => {
    const variables = { gameUuid };
    const query = BidsQuery
    const data = proxy.readQuery({ query, variables })
    data.game.bids = bid.bids;
    proxy.writeQuery({ query, variables, data })
};

// write the newly created project to the query cache
// http://dev.apollodata.com/core/read-and-write.html#updating-the-cache-after-a-mutation
export const passMutationUpdate = ({ gameUuid }) => (proxy, { data: { pass } }) => {
    const variables = { gameUuid };
    const query = BidsQuery
    const data = proxy.readQuery({ query, variables })
    data.game.bids = pass.bids;
    proxy.writeQuery({ query, variables, data })
};

const gameChangeSubscription = gql`
    subscription gameStateChanged($uuid: String!) {
      gameStateChanged(uuid: $uuid) {
        bids {
            ...bidFragment
        }
      }
    }
    ${bidFragment}
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