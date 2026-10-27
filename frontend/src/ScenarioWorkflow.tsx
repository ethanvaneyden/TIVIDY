import { Container } from "react-bootstrap";
import { useParams, useNavigate } from "react-router";
import "./Scenario.css";
import ContainerCard from "./ContainerCard";

function ScenarioWorkflow() {
  const { scenario: scenarioId, instanceId } = useParams();
  const navigate = useNavigate();

  if (!scenarioId || !instanceId) {
    return (
      <Container className="py-5">
        <h1 className="landing-header">Scenario not found</h1>
      </Container>
    );
  }

  return (
    <Container
      className="scenario-page py-5"
      data-scenario={scenarioId.toLowerCase()}
    >
      <button
        type="button"
        className="chooser-back scenario-workflow-back"
        onClick={() => navigate(`/${scenarioId}/instances`)}
      >
        {" "}
        <span aria-hidden="true">←</span>
        Choose {scenarioId} instance
      </button>
      <span className="scenario-eyebrow">{scenarioId}</span>
      <h1 className="landing-header">Welcome to the workflow</h1>
      <p className="scenario-description">You are working in instance <span className="instance-text">{instanceId}</span></p>
      
      <ContainerCard>
        
      </ContainerCard>
    </Container>
  );
}

export default ScenarioWorkflow;
