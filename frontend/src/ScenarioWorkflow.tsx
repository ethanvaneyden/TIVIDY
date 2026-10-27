import { Container } from 'react-bootstrap';
import { useParams } from 'react-router';

function ScenarioWorkflow() {
    const { scenario: scenarioId, instanceId } = useParams();

    if (!scenarioId || !instanceId) {
        return (
            <Container className="py-5">
                <h1 className="landing-header">Scenario not found</h1>
            </Container>
        );
    }

    return (
        <Container className="py-5">
            <p className="text-primary fw-semibold text-uppercase small mb-2">
                {scenarioId}
            </p>
            <h1 className="landing-header">Welcome to the workflow</h1>
            <h2 className="h4">{instanceId}</h2>
        </Container>
    );
}

export default ScenarioWorkflow;
