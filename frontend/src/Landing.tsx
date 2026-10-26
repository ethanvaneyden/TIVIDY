import { Container, Row, Col } from "react-bootstrap";
import "./Landing.css"
import WorkflowCard from "./WorkflowCard";

function Landing() {
    return (
        <Container className="py-5">
            <div className="d-flex flex-column align-items-center gap-3 mb-3">
                <h2 className="text-center landing-subtitle">
                    Workflow Management
                </h2>
                <h1 className="text-center landing-header">
                    Welcome to TIVIDY
                </h1>
            </div>


            <Row className="g-4 ">
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
                    ></WorkflowCard>
                </Col>
                <Col md={6}>
                    <WorkflowCard
                        title="Assignment Marking"
                        description="Streamline assignment assessment, coordinate marking, and deliver consistent feedback to students."
                        image="marking.jpg"
                        accent="violet"
                        features={[
                            'Organise assignment submissions',
                            'Coordinate marking and assessment',
                            'Track progress and feedback',
                        ]}
                    />
                </Col>
            </Row>
        </Container>
    );
}

export default Landing