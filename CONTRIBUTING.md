# Welcome

Welcome to the medInria Community! 

# Rules for medInria participants
- To propose a bundle of code use the Pull Request (PR) system. Pull request target branch should be master.
- Identify the nature of the change of your PR with appropriate title, description and if needed label or milestone.
- Never auto validate Pull-Requests.
- To report a bug, to open a discussion or to request an enhancement use Github Issues. Many labels are available to do this:
	- Bugs, three labels possible:
		- Critical (blocked a release, for example a crashing bug)
		- Major
		- Minor
	- Discussion (to ask something, to propose new architecture, to propose big changes …)
	- Enhancement (to propose a new behavior or a new standalone features)
	
# Here is a brief summary of how community is structured:
- medInria Users:            people using medInria.
- medInria Developers:       people programming into medInria, modifying the API, writing plugins.
- medInria Contributors:     medInria Developers which propose their changes in medInria code via PR.
- medInria Reviewers:        people reviewing and merging the PR. This group is validated by the **STC**.
- medInria Consortium:       research centers and companies willing to share the cost of medInria development.
- medInria Consortium Staff: administrators of medInria. This group is directed by the **EC**.

About the steering committees:
- **STC**: medInria Scientific and Technical Committee, defines the technical roadmap twice a year, validate the contribution rules, the Reviewers team and discuss every technical point in medInria.
- **EC**: medInria Executive Committee, decides on evolution of the membership contract, the communication policy and the priorities of the Consortium.

# Getting started

Please **read carefully** [the GitHub guide on Contributing to Open Source](https://guides.github.com/activities/contributing-to-open-source/). We also recommend you to check this [more detailed documentation on issues and pull requests](https://help.github.com/categories/collaborating-with-issues-and-pull-requests/).

# Issues

For **bug tracking**, **feature proposals** and **task management**, create a [medInria issue](https://github.com/medInria/medInria-public/issues)! Whatever issue you are having, you are likely not the only one, so others will find your issue helpful too. Issues labeled "Discussion" are also used for larger topics: architecture, future of medInria, long term dev, etc.

Please **DO NOT** create an issue for support. Consider reading the online help on the Github medInria Wiki to install prerequisites, known how to compile the software, or learn how to use it.

When creating an issue, pay attention to the following tips:

- **Check existing issues**. What you are running into may have been addressed already.
- **Set the right label** to your issue among our label list or propose them in the description.
- **Be clear** about what your problem is: what was the expected outcome, what happened instead? Detail how someone else can recreate the problem.

For more information on issues, check out [this GitHub guide](https://guides.github.com/features/issues/).  


# Pull requests

If you are able to patch the bug or add the feature yourself – fantastic, make a Pull Request with the code! Be sure you have read any documents on contributing and you understand the medInria license. Once you have submitted a PR the maintainer(s) can compare your branch to the existing one and decide whether or not to incorporate (pull in) your changes.

### Reminder - How to manage Pull Requests (from GitHub documentation)

- **[Fork](http://guides.github.com/activities/forking/)** the repository and clone it locally.
- [Connect your clone](https://help.github.com/articles/configuring-a-remote-for-a-fork/) to [the original **upstream** repository](https://github.com/medInria-framework/medInria/) by adding it as a remote.
- **Create a branch** for your changes.
- Make your changes.
- Pull in changes from upstream often to [**sync your fork**](https://help.github.com/articles/syncing-a-fork/) so that merge conflicts will be less likely in your pull request.
- [**Create a Pull Request**](https://help.github.com/articles/creating-a-pull-request-from-a-fork/) when you are ready to propose your changes into the main project.

### Rules

- Description must explain the **problem solved** or the **feature added**, and this must be reported in the **[RELEASE_NOTES.txt](https://github.com/medInria/medInria-public/blob/master/RELEASE_NOTES.txt)** file if the changes are meaningful.
- Code must follow **[our guidelines](https://github.com/medInria/medInria-public/blob/master/GUIDELINES.md)** (not done yet).
- Commit must build **successfully** on medInria integration system (Jenkins) for all steps: compilation + tests + examples.
- **Unit Tests** are highly recommended for each new component or if an issue is fixed (as soon as the Unit Tests system is created in medInria, and the documentation to do it is done).

### Lifecycle

Standard Pull Requests are reviewed and approved by the "Reviewers" team.
Major PR (BREAKING, major features) are reviewed by the "Reviewers" team and approved by the "STC members" team through a vote within a maximum period of 2 weeks.

Reviewing:

- Make sure the Pull Request has an appropriate title, description, and if needed milestone and labels.
- Control that it follows **our rules** defined above.
- **Control the builds**: Dashboard > Details, in the Pull Request "Checks" tab (if the system is installed).
- Merge method: **prefer "merge"** or "squash" over "rebase".

Remember that:

- You can **add commits** in a Pull Request: see [GitHub documentation](https://help.github.com/articles/committing-changes-to-a-pull-request-branch-created-from-a-fork/).
- If the pull request contains out of scope commits (from a previous merge with master), **consider rebasing it**.

For more information on forks and Pull Request, check out [this GitHub guide](https://guides.github.com/activities/forking/).
