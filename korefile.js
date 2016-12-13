var project = new Project('Exercise8', __dirname);

project.addFile('Sources/**');
project.setDebugDir('Deployment');

Project.createProject('Kore', __dirname).then((subproject) => {
	project.addSubProject(subproject);
	project.cpp11 = true;
	resolve(project);
});
