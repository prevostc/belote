import React from 'react';
import PropTypes from 'prop-types';
import JoinGame from '../game/JoinGame';
import {compose, pure, withState, withHandlers} from 'recompose';

const enhance = compose(
    withState('uuid', 'updateUuid', ''),
    withHandlers({
        onUuidChange: props => event => props.updateUuid(event.target.value),
    }),
    pure
);

export const JoinGameWithUuid = enhance(({ uuid, onUuidChange, onGameJoined }) => {
    return (
      <div>
          <input name="uuid" placeholder="uuid" onChange={onUuidChange} value={uuid} />
          <JoinGame gameUuid={uuid} />
      </div>
    );
});

JoinGameWithUuid.propTypes = {
    onGameJoined: PropTypes.func,
};

export default JoinGameWithUuid;
