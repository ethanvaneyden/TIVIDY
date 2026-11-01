import { Routes, Route } from 'react-router';

import Landing from '../pages/Landing';
import ScenarioInstanceSelect from '../pages/ScenarioInstanceSelect';
import ScenarioWorkflow from '../pages/ScenarioWorkflow';
import { Container } from 'react-bootstrap';
import { useNavigate } from 'react-router';

function App() {
    return (
        <Routes>
            <Route
                path="/"
                element={<Landing />}
            />

            <Route
                path="/:scenario/instances"
                element={<ScenarioInstanceSelect />}
            />
            <Route
                path="/:scenario/:instanceId"
                element={<ScenarioWorkflow />}
            />
            <Route
                path="*"
                element={<NotFound />}
            />
        </Routes>
    );
}

function NotFound() {
    const navigate = useNavigate();

    return (
        <Container className="scenario-page py-5">
            <p className="landing-eyebrow">TIVIDY WORKSPACE</p>
            <h1 className="landing-header">Page not found</h1>
            <p className="scenario-description">The page you&apos;re looking for doesn&apos;t exist.</p>
            <button type="button" className="cta-button" onClick={() => navigate('/')}>
                Back to workspaces
            </button>
        </Container>
    );
}

export default App;