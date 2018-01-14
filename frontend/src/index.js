import React from 'react';
import ReactDOM from 'react-dom';
import { ApolloProvider } from 'react-apollo';
import { ApolloClient } from 'apollo-client';
import { HttpLink } from 'apollo-link-http';
import { InMemoryCache } from 'apollo-cache-inmemory';
import './index.css';
import App from './App';
import registerServiceWorker from './registerServiceWorker';
import { API_URL } from './config';

const client = new ApolloClient({
    link: new HttpLink({ uri: API_URL }),
    cache: new InMemoryCache()
});

ReactDOM.render(
    <ApolloProvider client={client}>
        <App/>
    </ApolloProvider>,
    document.getElementById('root'),
);

registerServiceWorker();
