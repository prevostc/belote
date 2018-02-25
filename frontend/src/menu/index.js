import React from 'react';
import PropTypes from 'prop-types';
import CreateGame from './CreateGame';
import JoinGameWithUuid from './JoinGameWithUuid';
import {compose, pure} from "recompose";

const enhance = compose(
    pure
);

const Menu = enhance(({ playerUuid }) => {
    return (
        <div>
            <CreateGame onGameCreated={(uuid) => window.location.replace(`/game/${uuid}`)}/>
            <JoinGameWithUuid />
        </div>
    );
});

Menu.propTypes = {
    playerUuid: PropTypes.string.isRequired,
};

export default Menu