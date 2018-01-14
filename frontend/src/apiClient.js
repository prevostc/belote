import { ApolloClient } from 'apollo-client';
import { HttpLink } from 'apollo-link-http';
import { InMemoryCache } from 'apollo-cache-inmemory';
import { WebSocketLink } from 'apollo-link-ws';
import { getMainDefinition } from 'apollo-utilities';
import { split } from 'apollo-link';
import { API_URL, API_WS_URL } from './config';

// Create a WebSocket link:
const wsLink = new WebSocketLink({ uri: API_WS_URL, options: { reconnect: true } });

// Create an http link:
const httpLink = new HttpLink({ uri: API_URL });

// using the ability to split links, you can send data to each link
// depending on what kind of operation is being sent
const link = split(
    // split based on operation type
    ({ query }) => {
        const { kind, operation } = getMainDefinition(query);
        return kind === 'OperationDefinition' && operation === 'subscription';
    },
    wsLink,
    httpLink,
);

const cache = new InMemoryCache();

const client = new ApolloClient({ link, cache });

export default client;