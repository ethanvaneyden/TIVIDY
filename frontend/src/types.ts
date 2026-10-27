export interface Assignment {
    rootName : string,
    phase : string,
    items : Submission[]
}

export interface Submission {
    id : string,
    name : string,
    state : string,
    assignee : string
    reviewer : string
    reviewMethod : string

}