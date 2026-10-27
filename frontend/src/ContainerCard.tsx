import { Card, Badge } from "react-bootstrap";
import type { Assignment, Submission } from "./types"; // Assuming these exist
import SubmissionRow from "./SubmissionRow";
import "./ContainerCard.css";

// Improved mock data
const mock: Assignment = {
  rootName: "COS214 - Assignment 1",
  phase: "active",
  items: [
    {
      id: "SUB-001",
      name: "Liam van der Merwe",
      state: "assigned",
      assignee: "Mr Smith",
      reviewer: "",
      reviewMethod: "batch",
    },
    {
      id: "SUB-002",
      name: "Ayesha Khan",
      state: "completed",
      assignee: "Mr Jackson",
      reviewer: "Mrs Cook",
      reviewMethod: "batch",
    },
    {
      id: "SUB-003",
      name: "Thabo Mokoena",
      state: "rejected",
      assignee: "Mr Jackson",
      reviewer: "Mrs Cook",
      reviewMethod: "batch",
    },
    {
      id: "SUB-004",
      name: "Mei Chen",
      state: "inprogress",
      assignee: "Mr Jackson",
      reviewer: "",
      reviewMethod: "",
    },
    {
      id: "SUB-005",
      name: "Olivia Smith",
      state: "available",
      assignee: "",
      reviewer: "",
      reviewMethod: "",
    },
  ],
};

function ContainerCard() {


  return (
    <Card className="container-card">
      
      {/* Bootstrap Card Header */}
      <Card.Header className="d-flex justify-content-between align-items-center bg-white border-bottom pt-3 pb-3">
        <h4 className="container-header-title">{mock.rootName}</h4>
        
        {/* Phase Badge */}
        <Badge bg="primary" className="text-uppercase px-3 py-2">
          {mock.phase}
        </Badge>
      </Card.Header>

      {/* Card Body containing the list */}
      <Card.Body>
        {mock.items.map((item: Submission) => (
          <SubmissionRow 
            key={item.id} 
            {...item} 
          />
        ))}
      </Card.Body>

    </Card>
  );
}

export default ContainerCard;