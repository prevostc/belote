import React from 'react';
import {graphql} from "react-apollo/index";
import {
    bidMutation, bidMutationUpdate, joinGameMutation, joinGameMutationUpdate, gameQuery, passMutation,
    passMutationUpdate, gameChangeSubscription, playCardMutation, playCardMutationUpdate, createGameMutation,
    createGameMutationUpdate
} from "./query";
import {branch, compose, lifecycle, renderComponent, setPropTypes} from "recompose";
import PropTypes from "prop-types";

export const connectBidsMutations = compose(
    setPropTypes({
        gameUuid: PropTypes.string.isRequired,
        playerUuid: PropTypes.string.isRequired,
    }),
    graphql(bidMutation, {
        name: 'bid',
        props: ({ bid, ownProps: { gameUuid, playerUuid } }) => ({
            bid: (value, color) => {
                const variables = { gameUuid, playerUuid, value, color };
                return bid({
                    variables,
                    update: bidMutationUpdate(variables),
                });
            }
        }),
    }),
    graphql(passMutation, {
        name: 'pass',
        props: ({ pass, ownProps: { gameUuid, playerUuid } }) => ({
            pass: () => {
                const variables = { gameUuid, playerUuid };
                return pass({
                    variables,
                    update: passMutationUpdate(variables),
                });
            }
        }),
    }),
);

export const connectPlayCardMutation = compose(
    setPropTypes({
        gameUuid: PropTypes.string.isRequired,
        playerUuid: PropTypes.string.isRequired,
    }),
    graphql(playCardMutation, {
        name: 'playCard',
        props: ({ playCard, ownProps: { gameUuid, playerUuid } }) => ({
            playCard: (color, motif) => {
                const variables = { gameUuid, playerUuid, color, motif };
                return playCard({
                    variables,
                    update: playCardMutationUpdate(variables),
                });
            }
        }),
    })
);

export const connectJoinGameMutation = compose(
    setPropTypes({
        gameUuid: PropTypes.string.isRequired,
        playerUuid: PropTypes.string.isRequired,
        onGameJoined: PropTypes.func,
    }),
    graphql(joinGameMutation, {
        name: 'joinGame',
        props: ({ joinGame, ownProps: { playerUuid, gameUuid, onGameJoined } }) => ({
            joinGame: async (name, spot) => {
                const variables = { uuid: gameUuid, playerUuid, name, spot };
                const data = await joinGame({
                    variables,
                    update: joinGameMutationUpdate(variables),
                });
                onGameJoined && data.data.joinGame && onGameJoined({
                    uuid: data.data.joinGame.uuid,
                    name: data.data.joinGame.name,
                    spot: data.data.joinGame.spot,
                });
            }
        }),
    })
);

export const connectCreateGameMutation = compose(
    setPropTypes({
        onGameCreated: PropTypes.func.isRequired,
    }),
    graphql(createGameMutation, {
        name: 'createGame',
        props: ({ createGame, ownProps: { onGameCreated } }) => ({
            createGame: async (name) => {
                const variables = {name};
                const data = await createGame({
                    variables,
                    update: createGameMutationUpdate(variables),
                });
                onGameCreated(data.data.createGame.uuid);
            }
        }),
    })
);

export const enhanceWithGame = (mapper) => {
    let unsubscribe = null;

    return compose(
        setPropTypes({
            gameUuid: PropTypes.string.isRequired,
            playerUuid: PropTypes.string.isRequired,
        }),
        graphql(gameQuery, {
            props: ({ data }) => {
                const { loading, error, game } = data;
                const res = {
                    loading,
                    error,
                    subscribeToChange: ({playerUuid, gameUuid}) => data.subscribeToMore({
                        document: gameChangeSubscription,
                        variables: {playerUuid, gameUuid},
                        updateQuery: (prev, {subscriptionData}) => {
                            if (!subscriptionData.data) {
                                return prev;
                            }
                            const newGame = subscriptionData.data.gameStateChanged;
                            return {
                                ...prev,
                                game: newGame
                            };
                        },
                    })
                };
                return game ? { ...res, ...mapper(game) } : res;
            },
            options: ({ gameUuid, playerUuid }) => ({
                variables: { gameUuid, playerUuid },
            }),
        }),
        lifecycle({
            componentDidUpdate(prevProps) {
                const propsChanged = this.props.gameUuid !== prevProps.gameUuid || this.props.playerUuid !== prevProps.playerUuid;
                if (!unsubscribe || propsChanged) {
                    unsubscribe && unsubscribe();
                    unsubscribe = this.props.subscribeToChange({
                        gameUuid: this.props.gameUuid,
                        playerUuid: this.props.playerUuid,
                    });
                }
            }
        }),
        branch(
            props => props.loading,
            renderComponent(() => <div>LOADING</div>),
        ),
        branch(
            props => props.error,
            renderComponent(({ error }) => <div>ERROR: {JSON.stringify(error)}</div>),
        )
    )
};