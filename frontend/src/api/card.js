import gql from "graphql-tag";
import {gameFragment} from "./game";

export const cardFragment = gql`
    fragment cardFragment on Card {
        color
        motif
    }
`;

export const CardsQuery = gql`
    query game($gameUuid: String!, $playerUuid: String!) {
        game(uuid: $gameUuid) {
            cards (playerUuid: $playerUuid) {
                ...cardFragment
            }
        }
    }

    ${cardFragment}
`;

export const playCardMutation = gql`
    mutation playCard($gameUuid: String!, $playerUuid: String!, $color: CardColor!, $motif: CardMotif!) {
        playCard(gameUuid: $gameUuid, playerUuid: $playerUuid, color: $color, motif: $motif) {
            cards (playerUuid: $playerUuid) {
                ...cardFragment
            }
        }
    }
    ${cardFragment}
`;

// write the newly created project to the query cache
// http://dev.apollodata.com/core/read-and-write.html#updating-the-cache-after-a-mutation
export const playCardMutationUpdate = (variables) => (proxy, { data: { playCard } }) => {
    const query = CardsQuery
    const data = proxy.readQuery({ query, variables })
    data.game.cards = playCard.cards;
    proxy.writeQuery({ query, variables, data })
};

const gameChangeSubscription = gql`
    subscription gameStateChanged($gameUuid: String!, $playerUuid: String!) {
        gameStateChanged(uuid: $uuid) {
            ...gameFragment
            cards (playerUuid: $playerUuid) {
                ...cardFragment
            }
        }
    }
    ${cardFragment}
    ${gameFragment}
`;

export const subscribeToChange = (query) => ({gameUuid, playerUuid}) => {
    return query.subscribeToMore({
        document: gameChangeSubscription,
        variables: { gameUuid, playerUuid },
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