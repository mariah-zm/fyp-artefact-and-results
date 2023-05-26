#include "calendar.ih"

Calendar::Interval const *Calendar::nextScheduledTask(double currentTime) const
{
    return nextInterval(d_root, currentTime);
}

void Calendar::addEvent(size_t start, size_t end, Calendar::Task task)
{
    Interval interval = {start, end, task};
    d_root = insertInterval(d_root, interval);
}

Calendar::Task const *Calendar::searchCalendar(double start, double end) const
{
    auto interval = searchInterval(d_root, start, end);

    if (interval == nullptr)
        return nullptr;
    else
        return &interval->task;
}

void Calendar::destroyNode(Calendar::Node *node)
{
    if (node != nullptr)
    {
        destroyNode(node->left);
        destroyNode(node->right);
        delete node;
    }
}

Calendar::Node *Calendar::insertInterval(Calendar::Node *root, Calendar::Interval interval)
{
    // Base case
    if (root == nullptr)
    {
        return new Node(interval);
    }

    // TODO: Add logic for when added event overlaps root

    // If the given interval has a start time less than the root node's interval start time, insert it into the left subtree
    if (interval.start < root->interval.start)
        root->left = insertInterval(root->left, interval);
    // Otherwise, insert it into the right subtree
    else
        root->right = insertInterval(root->right, interval);

    // Update the maximum end time of intervals in the subtree rooted at this node
    if (root->maxEnd < interval.end)
        root->maxEnd = interval.end;

    return root;
}

Calendar::Interval *Calendar::nextInterval(Calendar::Node *root, double currentTime) const
{
    // Base case
    if (root == nullptr)
        return nullptr;

    // If the root has start time greater than currentTime, next interval is either the root or one on the left
    if (currentTime <= root->interval.start)
    {
        Interval *next = &root->interval;
        Interval *possibleNext = nextInterval(root->left, currentTime);
        if (possibleNext != nullptr)
            return possibleNext;
        else 
            return next;
    }
    else 
        return nextInterval(root->right, currentTime);
}

Calendar::Interval *Calendar::searchInterval(Calendar::Node *root, double start, double end) const
{
    // Base case
    if (root == nullptr)
        return nullptr;

    // If the root interval overlaps with the search interval, return the root interval
    if (start <= root->interval.end && root->interval.start <= end)
        return &root->interval;

    // If the left subtree has intervals that end after the start time of the search interval,
    // search the left subtree
    if (root->left != nullptr && root->left->maxEnd >= start)
    {
        return searchInterval(root->left, start, end);
    }

    // Otherwise, search the right subtree
    return searchInterval(root->right, start, end);
}

void Calendar::print(ofstream &out, Calendar::Node *root) const
{
    if (root)
    {
        out << root->interval.start << '-' << root->interval.end << ": " << root->interval.task.task << ',';
        print(out, root->left);
        print(out, root->right);
    }
}