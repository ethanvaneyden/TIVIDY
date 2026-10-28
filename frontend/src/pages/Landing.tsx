import { Container, Row, Col } from "react-bootstrap";
import '../styles/Landing.css';
import WorkflowCard from '../components/home/WorkflowCard';

function Landing() {
    return (
        <Container className="landing-page py-5">
            <header className="landing-intro">
                <span className="landing-eyebrow">
                    <span className="landing-eyebrow-dot" />
                    TIVIDY WORKSPACE
                </span>
                <h1 className="landing-header">
                    Your workflows,<br />
                    <span>moving forward.</span>
                </h1>
                <p className="landing-lede">
                    A clearer way to coordinate the work that matters.
                    Choose a workspace and start a workflow.
                </p>
            </header>

            <div className="workflow-section-heading">
                <div>
                    <p className="workflow-section-kicker">YOUR WORKSPACES</p>
                    <h2>Choose a workflow</h2>
                </div>
                <span className="workflow-count">02 WORKSPACES</span>
            </div>

            <Row className="g-4 workflow-grid">
                <Col md={6}>
                    <WorkflowCard title="Patient Admissions"
                        description="Manage patient admissions efficiently."
                        image="hospital.jpg"
                        accent="blue"
                        features={[
                            'Manage patient information',
                            'Track admission status',
                            'Organise patient workflows',
                        ]}
                        link="/healthcare/instances"
                    />
                </Col>
                <Col md={6}>
                    <WorkflowCard
                        title="Assignment Marking"
                        description="Streamline assignment assessment, coordinate marking, and deliver consistent feedback to students."
                        image="marking.jpg"
                        accent="purple"
                        features={[
                            'Organise assignment submissions',
                            'Coordinate marking and assessment',
                            'Track progress and feedback',
                        ]}
                        link="/education/instances"
                    />
                </Col>
            </Row>
        </Container>
    );
}

export default Landing