import React from 'react';
import { graphql } from 'react-apollo';
import PropTypes from 'prop-types';
import {compose, pure, withState, withHandlers, renderComponent, branch} from 'recompose';
import { joinGameQuery, joinGameUpdate } from "../api/game";

const enhance = compose(
    graphql(joinGameQuery, {
        name: 'joinGame',
        props: ({ joinGame, ownProps: { playerUuid, onGameJoined, gameUuid } }) => ({
            joinGame: async (name, spot) => {
                const variables = { uuid: gameUuid, playerUuid, name, spot };
                console.log(variables);
                const data = await joinGame({
                    variables,
                    update: joinGameUpdate({ uuid: gameUuid }),
                })
                onGameJoined && onGameJoined({
                    uuid: data.data.joinGame.uuid,
                    name: data.data.joinGame.name,
                    spot: data.data.joinGame.spot,
                });
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
