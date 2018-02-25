import React from 'react';
import PropTypes from 'prop-types';
import {compose, pure, withState, withHandlers} from 'recompose';
import {connectJoinGameMutation} from "../api";

const enhance = compose(
    connectJoinGameMutation,
    withState('name', 'updateName', ''),
    withHandlers({
        onNameChange: props => event => props.updateName(event.target.value),
    }),
    pure
);

// @todo: get those from the engine lib
const spots = ['NORTH', 'EAST', 'SOUTH', 'WEST'];

export const JoinGame = enhance(({ joinGame, errors, name, onNameChange }) => {
    return (
      <div>
        <input name="name" placeholder="name" onChange={onNameChange} value={name} />
        {spots.map(spot => <button key={spot} onClick={() => joinGame(name, spot)}>Join {spot}</button>)}
        <ul>{errors && errors.map(e => <li>{e}</li>)}</ul>
      </div>
    );
});

JoinGame.propTypes = {
    gameUuid: PropTypes.string.isRequired,
    onGameJoined: PropTypes.func,
    playerUuid: PropTypes.string,
};

export default JoinGame;
