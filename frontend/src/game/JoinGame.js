import React from 'react';
import { graphql } from 'react-apollo';
import gql from 'graphql-tag';
import PropTypes from 'prop-types';
import {compose, pure, withState, withHandlers, renderComponent, branch} from 'recompose';

const enhance = compose(
    graphql(gql`
            mutation joinGame($gameUuid: String!, $name: String!) {
                createGame(gameUuid: $gameUuid, name: $name) {
                    uuid
                    name
                    spot
                    game {
                        uuid
                        gameState
                    }
                }
            }
        `, {
        name: 'joinGame',
        props: ({ joinGame, ownProps: { onGameJoined } }) => ({
            joinGame: async (uuid, name) => {
                const data = await joinGame({ variables: { uuid, name } })
                onGameJoined && onGameJoined(data.data.createGame.uuid);
            }
        }),
    }),
    branch(
        props => props.loading,
        renderComponent(() => <div>LOADING</div>),
    ),
    branch(
        props => props.error,
        renderComponent(({ error }) => <div>ERROR: {JSON.stringify(error)}</div>),
    ),
    withState('name', 'updateName', ''),
    withHandlers({
        onNameChange: props => event => props.updateName(event.target.value),
    }),
    pure
);

export const JoinGame = enhance(({ joinGame, errors, gameUuid, name, onNameChange }) => {
    return (
      <div>
        <input name="name" placeholder="name" onChange={onNameChange} value={name} />
        <button onClick={() => joinGame(gameUuid, name)}>Join Game</button>
        <ul>{errors && errors.map(e => <li>{e}</li>)}</ul>
      </div>
    );
});

JoinGame.propTypes = {
    gameUuid: PropTypes.string.isRequired,
    onGameJoined: PropTypes.func,
};

export default JoinGame;
