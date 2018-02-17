import React from 'react';
import { graphql } from 'react-apollo';
import {compose, pure, branch, renderComponent, lifecycle, withState} from 'recompose';
import JoinGame from './JoinGame';
import BidPhase from './BidPhase';
import { gameQuery, subscribeToChange } from "../api/game";


const enhance = compose(
    graphql(gameQuery, {
        props: ({ data }) => {
            const { loading, error, game } = data;
            return ({
                loading,
                error,
                game,
                subscribeToChange: subscribeToChange(data),
            })
        },
        options: ({ uuid }) => ({
            variables: { uuid },
        }),
    }),
    lifecycle({
        componentWillMount() {
            this.props.subscribeToChange({
                uuid: this.props.uuid,
            });
        }
    }),
    branch(
        props => props.loading,
        renderComponent(() => <div>LOADING</div>),
    ),
    branch(
        props => props.error,
        renderComponent(({ error }) => <div>ERROR: {JSON.stringify(error)}</div>),
    ),
    withState('player', 'setPlayer', ''),
    pure
);

export const Board = enhance(({ player, setPlayer, game: { uuid, phase, gameState, players }}) => {
    const p = player ? <div>{player.uuid} - {player.name} - {player.spot}</div> : <div></div>;
    return (
      <div>
          {p}
          <div>{uuid}</div>
          <pre>{gameState}</pre>
          {players.map(({name, uuid, spot, isDealer}) => <div key={spot}>{uuid} - {name} - {spot}{isDealer ? ' - DEALER': ''}</div>)}
          { phase === 'INITIAL'
              ? <JoinGame playerUuid={player ? player.uuid : null} onGameJoined={setPlayer} gameUuid={uuid}/>
              : <BidPhase/>
          }

      </div>
    );
});

export default Board;
