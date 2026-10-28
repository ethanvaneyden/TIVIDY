import ReactDOM from 'react-dom/client';
import { BrowserRouter } from 'react-router';

import App from './app/App';

import 'bootstrap/dist/css/bootstrap.min.css';
import './styles/index.css';

ReactDOM.createRoot(document.getElementById('root')!).render(
    <BrowserRouter>
        <App />
    </BrowserRouter>
);