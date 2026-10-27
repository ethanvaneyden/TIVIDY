import { Container } from 'react-bootstrap';
import { useParams } from 'react-router';
import './Scenario.css';

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
        <Container className="scenario-page py-5" data-scenario={scenarioId.toLowerCase()}>
            <span className="scenario-eyebrow">{scenarioId}</span>
            <h1 className="landing-header">Welcome to the workflow</h1>
            <p className="scenario-description">You are working in instance</p>
            <div className="scenario-instance-id">{instanceId}</div>
        </Container>
    );
}

export default ScenarioWorkflow;
