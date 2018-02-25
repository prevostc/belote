import React from 'react';
import PropTypes from 'prop-types';
import {compose, pure, withState, withHandlers, renderComponent, branch} from 'recompose';
import {connectCreateGameMutation} from "../api";

const enhance = compose(
    connectCreateGameMutation,
    branch(
        props => props.loading,
        renderComponent(() => <div>LOADING</div>),
    ),
    branch(
        props => props.error,
        renderComponent(({ error }) => <div>ERROR: {JSON.stringify(error)}</div>),
    ),
    withState('value', 'updateValue', ''),
    withHandlers({
      onChange: props => event => props.updateValue(event.target.value),
    }),
    pure
);

export const CreateGame = enhance(({ createGame, errors, value, onChange }) => {
    return (
      <div>
        <input type="name" placeholder="name" onChange={onChange} value={value} />
        <button onClick={() => createGame(value)}>Create Game</button>
        <ul>{errors && errors.map(e => <li>{e}</li>)}</ul>
      </div>
    );
});

CreateGame.propTypes = {
    onGameCreated: PropTypes.func.isRequired,
};

export default CreateGame;
