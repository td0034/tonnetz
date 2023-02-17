'use strict';(function(){var win;if(typeof window!=="undefined"){win=window;if(win.$_mod)return;win.global=win}var $_mod;var definitions=Object.create(null);var searchPaths=[];var _ready=false;var runQueue=[];var instanceCache=Object.create(null);var installed=Object.create(null);var builtins=Object.create(null);var mains=Object.create(null);var remapped=Object.create(null);function moduleNotFoundError(target,from){var err=new Error('Cannot find module "'+target+'"'+(from?' from "'+from+'"':""));err.code="MODULE_NOT_FOUND";return err}function Module(filename){this.id=this.filename=filename;this.loaded=false;this.exports=undefined}Module.cache=instanceCache;var Module_prototype=Module.prototype;Module_prototype.load=function(factoryOrObject){var filename=this.id;if(typeof factoryOrObject==="function"){var dirname=filename.slice(0,filename.lastIndexOf("/"));var instanceRequire=function(target){return require(target,dirname)};instanceRequire.resolve=function(target){if(!target)throw moduleNotFoundError("");var resolved=resolve(target,dirname);if(resolved===undefined)throw moduleNotFoundError(target,dirname);return resolved};instanceRequire.cache=instanceCache;instanceRequire.runtime=$_mod;this.exports={};factoryOrObject(instanceRequire,this.exports,this,filename,dirname)}else this.exports=factoryOrObject;this.loaded=true};function define(path,factoryOrObject,options){var globals=options&&options.globals;definitions[path]=factoryOrObject;if(globals){var target=win||global;var globalMod=require(path,"/");for(var i=0;i<globals.length;i++)target[globals[i]]=globalMod}}function registerMain(path,relativePath){mains[path]=relativePath}function remap(fromPath,toPath){remapped[fromPath]=toPath}function builtin(name,target){builtins[name]=target}function registerInstalledDependency(parentPath,packageName,packageVersion){installed[parentPath+"/"+packageName]=packageVersion}function join(from,target){var fromLen=from.length;var fromLastIndex=fromLen;var targetStartIndex=0;var char;
while((char=target[targetStartIndex])==="."){targetStartIndex++;if((char=target[targetStartIndex])==="."){targetStartIndex++;if(fromLastIndex){fromLastIndex=from.lastIndexOf("/",fromLastIndex-1);if(fromLastIndex===-1)fromLastIndex=0}}if((char=target[targetStartIndex])==="/")targetStartIndex++;else break}if(char){if(fromLastIndex)return from.slice(0,fromLastIndex)+"/"+target.slice(targetStartIndex);return target.slice(targetStartIndex)}if(fromLastIndex)return fromLastIndex===fromLen?from:from.slice(0,fromLastIndex);return from[0]==="/"?"/":"."}function withoutExtension(path){var lastDotPos=path.lastIndexOf(".");var lastSlashPos;return lastDotPos===-1||(lastSlashPos=path.lastIndexOf("/"))!==-1&&lastSlashPos>lastDotPos?undefined:path.substring(0,lastDotPos)}function splitPackageIdAndSubpath(path){path=path.substring(1);var slashPos=path.indexOf("/");if(path[1]==="@")slashPos=path.indexOf("/",slashPos+1);var packageIdEnd=slashPos===-1?path.length:slashPos;return[path.substring(0,packageIdEnd),path.substring(packageIdEnd)]}function resolveInstalledModule(target,from){if(target[target.length-1]==="/")target=target.slice(0,-1);var builtinPath=builtins[target];if(builtinPath)return builtinPath;var fromParts=splitPackageIdAndSubpath(from);var fromPackageId=fromParts[0];var targetSlashPos=target.indexOf("/");var targetPackageName;var targetSubpath;if(targetSlashPos<0){targetPackageName=target;targetSubpath=""}else{if(target[0]==="@")targetSlashPos=target.indexOf("/",targetSlashPos+1);targetPackageName=target.substring(0,targetSlashPos);targetSubpath=target.substring(targetSlashPos)}var targetPackageVersion=installed[fromPackageId+"/"+targetPackageName];if(targetPackageVersion){var resolvedPath="/"+targetPackageName+"$"+targetPackageVersion;if(targetSubpath)resolvedPath+=targetSubpath;return resolvedPath}}function resolve(target,from){var resolvedPath;if(target[0]==="/")resolvedPath=target;else if(target[0]===".")resolvedPath=join(from,target);else{var len=searchPaths.length;for(var i=
0;i<len;i++){var candidate=searchPaths[i]+target;var resolved=resolve(candidate,from);if(resolved)return resolved}resolvedPath=resolveInstalledModule(target,from)}if(!resolvedPath)return undefined;var relativePath=mains[resolvedPath];if(relativePath!==undefined)resolvedPath=join(resolvedPath,relativePath||"index");var remappedPath=remapped[resolvedPath];if(remappedPath)resolvedPath=remappedPath;if(definitions[resolvedPath]===undefined){resolvedPath=withoutExtension(resolvedPath);if(resolvedPath!==undefined&&definitions[resolvedPath]===undefined)resolvedPath=undefined}return resolvedPath}function requireModule(target,from){if(!target)throw moduleNotFoundError("");var resolvedPath=resolve(target,from);if(resolvedPath===undefined)throw moduleNotFoundError(target,from);var module=instanceCache[resolvedPath];if(module===undefined){module=instanceCache[resolvedPath]=new Module(resolvedPath);module.load(definitions[resolvedPath])}return module}function require(target,from){return requireModule(target,from).exports}function run(path,options){var wait=!options||options.wait!==false;if(wait&&!_ready)return runQueue.push([path,options]);requireModule(path,"/")}function ready(){_ready=true;var len;while(len=runQueue.length){var queue=runQueue;runQueue=[];for(var i=0;i<len;i++){var args=queue[i];run(args[0],args[1])}if(!_ready)break}}function addSearchPath(prefix){searchPaths.push(prefix)}var pendingCount=0;var onPendingComplete=function(){pendingCount--;if(!pendingCount)ready()};Module_prototype.__runtime=$_mod={def:define,installed:registerInstalledDependency,run:run,main:registerMain,remap:remap,builtin:builtin,require:require,resolve:function(target,from){var resolved=resolve(target,from);if(resolved!==undefined)return[resolved,definitions[resolved]]},join:join,ready:ready,searchPath:addSearchPath,loaderMetadata:function(data){Module_prototype.__loaderMetadata=data},pending:function(){_ready=false;pendingCount++;return{done:onPendingComplete}}};if(win)win.$_mod=$_mod;else module.exports=
$_mod})();'use strict';$_mod.def("/sgninui$2.0.0/src/pages/sub/obfuscatedAlt.marko.init",function(require,exports,module,__filename,__dirname){window.$initComponents&&window.$initComponents()});'use strict';$_mod.run("/sgninui$2.0.0/src/pages/sub/obfuscatedAlt.marko.init");